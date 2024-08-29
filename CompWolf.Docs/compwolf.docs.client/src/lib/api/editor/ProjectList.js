'use client'

import { useState } from "react"
import { postProject } from "@/lib/api/getEntity"
import Input, { SubmitInput } from "@/lib/Input"
import { Reference } from "@/lib/CodeComponents"

export default function ApiEditorPage(parameters) {
	const [data, setData] = useState(parameters.data)

	const [newProjectName, setNewProjectName] = useState("")
	const [newProjectProcessing, setNewProjectProcessing] = useState(false)

	function onProjectSubmit(event) {
		setNewProjectProcessing(true)
		event.preventDefault()

		const postData = {
			briefDescription: "",
			detailedDescription: []
		}
		const newProjectData = {
			name: newProjectName,
			headers: [],
			...postData
		}
		console.log(newProjectData)

		postProject(newProjectName, postData).then(result => {
			if (!result) {
				alert("Could not save data")
			} else {
				setData({
					...data,
					projects: [...data.projects, newProjectData]
				})
				setNewProjectName("")
			}
			setNewProjectProcessing(false)
		})
	}

	return (
		<section>
			<h2>Projects:</h2>
			<ul>
				{data.projects.map((x, i) => {
					return (
						<li key={i}>
							<Reference path={`/editor/api/${x.name}`}>{x.name}</Reference>: {x.briefDescription}
						</li>
					)
				})}
				<li>
					<form onSubmit={onProjectSubmit}>
						New:
						<Input setter={setNewProjectName} value={newProjectName} disabled={newProjectProcessing} />
						<SubmitInput label="Create" disabled={newProjectProcessing} />
					</form>
				</li>
			</ul>
		</section>
	)
}
