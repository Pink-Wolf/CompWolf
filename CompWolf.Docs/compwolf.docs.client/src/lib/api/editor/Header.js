'use client'

import { useState } from "react"
import { postHeader, postEntity } from "@/lib/api/getEntity"
import Input, { SubmitInput } from "@/lib/Input"
import { Reference } from "@/lib/CodeComponents"
import betterEncodeURIComponent from "@/lib/betterEncodeURIComponent"

export default function HeaderEditor(parameters) {
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

	const [newEntityName, setNewEntityName] = useState("")
	const [newEntityProcessing, setNewEntityProcessing] = useState(false)

	function onSubmit(event) {
		setProcessing(true)
		event.preventDefault()

		const {
			project: project,
			header: header,
			entities: entities,
			...output
		} = data
		console.log(output)

		postHeader(project, header, output).then(result => {
			if (!result) {
				alert("Could not save data")
			} else {
				setIsSaved(true)
			}
			setProcessing(false)
		})
	}

	function onEntitySubmit(event) {
		setNewEntityProcessing(true)
		event.preventDefault()

		const postData = {
			namespace: "",
			briefDescription: "",
			detailedDescription: []
		}
		const newEntityData = {
			type: "undefined",
			name: newEntityName,
			...postData
		}
		console.log(newEntityData)

		postEntity(data.project, data.header, newEntityName, postData).then(result => {
			if (!result) {
				alert("Could not save data")
			} else {
				setData({
					...data,
					entities: [...data.entities, newEntityData]
				})
				setNewEntityName("")
			}
			setNewEntityProcessing(false)
		})
	}

	return (
		<section>
			<form onSubmit={onSubmit}>
				<small>
					<p id="Project">In project <Reference path={`/editor/api/${data.project}`}>{data.project}</Reference></p>
				</small>
				<h1 id="Declaration"> {`${data.header}`} </h1>
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

			<h2>Entities:</h2>
			<ul>
				{data.entities.map((x, i) => {
					return (
						<li key={i}>
							<Reference path={`/editor/api/${data.project}/${data.header}/${betterEncodeURIComponent(x.name)}`}>{x.name}</Reference>: {x.briefDescription}
						</li>
					)
				})}
				<li>
					<form onSubmit={onEntitySubmit}>
						New:
						<Input setter={setNewEntityName} value={newEntityName} disabled={newEntityProcessing} />
						<SubmitInput label="Create" disabled={newEntityProcessing} />
					</form>
				</li>
			</ul>
		</section>
	)
}
