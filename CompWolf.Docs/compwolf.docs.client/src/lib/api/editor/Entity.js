'use client'

import { Fragment, useState } from "react"
import { postEntity } from "@/lib/api/getEntity"
import Input, { SubmitInput } from "@/lib/Input"
import { Reference } from "@/lib/CodeComponents"

import MutableTypeEditor from "./entities/MutableType"
import { defaultBaseEntity } from "./entities/BaseEntity"

export default function EntityEditor(parameters) {
	const [data, _setData] = useState({
		namespace: "",
		...defaultBaseEntity,
		...parameters.data
	})
	const [processing, setProcessing] = useState(false)

	const [isSaved, setIsSaved] = useState(true)
	function setData(x) {
		_setData({
			...x,
			project: data.project,
			header: data.header,
			entity: data.entity,
			owners: data.owners,
		})
		setIsSaved(false)
	}

	function onSubmit(event) {
		setProcessing(true)
		event.preventDefault()

		const {
			project: project,
			header: header,
			entity: entity,
			owners: owners,
			name: name,
			...output
		} = data
		console.log(output)

		postEntity(project, header, entity, output).then(result => {
			if (!result) {
				alert("Could not save data")
			} else {
				setIsSaved(true)
			}
			setProcessing(false)
		})
	}

	return (
		<form onSubmit={onSubmit}>
			<small>
				<p id="Project">In project <Reference path={`/editor/api/${data.project}`}>{data.project}</Reference></p>
				<p id="Header">Defined in header <Reference path={`/editor/api/${data.project}/${data.header}`}>&lt;{data.header}&gt;</Reference></p>
			</small>

			<Input field="namespace" setter={setData} value={data}
				disabled={processing}
			/>

			<MutableTypeEditor data={data} setData={setData}
				disabled={processing} immutableName
			/>

			<p id="saveInformation">{isSaved ? "All data saved" : "You have unsaved data"}</p>
			<SubmitInput label="Save" disabled={processing} />
		</form>
	)
}
