'use client'

import { useState } from "react"
import { postProject, postHeader } from "@/lib/api/getEntity"
import Input, { SubmitInput } from "@/lib/Input"
import { Reference } from "@/lib/CodeComponents"
import FormattedText from "@/lib/FormattedText"

export default function ProjectEditor(parameters) {
	const [data, _setData] = useState({
		briefDescription: "",
		detailedDescription: [],
		...parameters.data
	})
	const [processing, setProcessing] = useState(false)

	const [isSaved, setIsSaved] = useState(true)
	function setData(x) {
		_setData(x)
		setIsSaved(false)
	}

	const [newHeaderName, setNewHeaderName] = useState("")
	const [newHeaderProcessing, setNewHeaderProcessing] = useState(false)

	function onSubmit(event) {
		setProcessing(true)
		event.preventDefault()

		const {
			project: project,
			headers: headers,
			...output
		} = data
		console.log(output)

		postProject(project, output).then(result => {
			if (!result) {
				alert("Could not save data")
			} else {
				setIsSaved(true)
			}
			setProcessing(false)
		})
	}

	function onHeaderSubmit(event) {
		setNewHeaderProcessing(true)
		event.preventDefault()

		const postData = {
			briefDescription: "",
			detailedDescription: []
		}
		const newHeaderData = {
			name: newHeaderName,
			entities: [],
			...postData
		}
		console.log(newHeaderData)

		postHeader(data.project, newHeaderName, postData).then(result => {
			if (!result) {
				alert("Could not save data")
			} else {
				setData({
					...data,
					headers: [...data.headers, newHeaderData]
				})
				setNewHeaderName("")
			}
			setNewHeaderProcessing(false)
		})
	}

	return (
		<section>
			<form onSubmit={onSubmit}>
				<h1 id="Declaration"> {`${data.project}`} </h1>
				<Input field="briefDescription" type="textarea" setter={setData} value={data} disabled={processing} />
				<Input field="detailedDescription" type="array" setter={setData} value={data} disabled={processing}
					forEach={(x, i, setter) => {
						return (
							<Input type="textarea" value={x} setter={setter} disabled={processing} />
						)
					}}
				/>
				<p id="saveInformation">{isSaved ? "All data saved" : "You have unsaved data"}</p>
				<SubmitInput label="Save" disabled={processing} />
			</form>

			<h2>Header-files:</h2>
			<ul>
				{data.headers.map((x, i) => {
					return (
						<li key={i}>
							<Reference path={`/editor/api/${data.project}/${x.name}`}>{x.name}</Reference>: <FormattedText>{x.briefDescription}</FormattedText>
						</li>
					)
				})}
				<li>
					<form onSubmit={onHeaderSubmit}>
						New:
						<Input setter={setNewHeaderName} value={newHeaderName} disabled={newHeaderProcessing} />
						<SubmitInput label="Create" disabled={newHeaderProcessing} />
					</form>
				</li>
			</ul>
		</section>
	)
}
