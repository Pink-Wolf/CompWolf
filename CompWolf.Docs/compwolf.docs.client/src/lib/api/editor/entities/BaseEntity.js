'use client'

import Style from "@/styles/EntityViewer.css"

import { Fragment, useState } from "react"
import { useRouter } from "next/navigation"
import { postHeader } from "@/lib/api/getEntity"
import Input, { SubmitInput } from "@/lib/Input"
import { Reference } from "@/lib/CodeComponents"
import betterEncodeURIComponent from "@/lib/betterEncodeURIComponent"

export const defaultBaseEntity = {
	type: "undefined",
	warnings: [],
	declaration: "",
	briefDescription: "",
	detailedDescription: [],
	example: {
		code: "",
		description: ""
	},
	related: []
}

export default function BaseEntityEditor({
	data, setData,
	disabled, immutableName,
	top, children,
}) {

	return (
		<section>
			<button type="button" onClick={() => navigator.clipboard.writeText(JSON.stringify(data))}>Copy</button>
			<button type="button" onClick={async () => setData(JSON.parse(await navigator.clipboard.readText()))}>Paste</button>

			{top}

			{
				immutableName ? null : <Input field="name"
					setter={setData} value={data} disabled={disabled}
				/>
			}

			<h3>Description</h3>

			<Input field="briefDescription" type="textarea"
				setter={setData} value={data} disabled={disabled}
			/>

			<Input field="detailedDescription" type="array"
				setter={setData} value={data} disabled={disabled}
				forEach={(x, i, setter) => {
					return (
						<Input type="textarea"
							value={x} setter={setter} disabled={disabled}
						/>
					)
				}}
			/>

			<h3>Warnings</h3>

			<Input field="warnings" label="" type="array"
				setter={setData} value={data} disabled={disabled}
				forEach={(x, i, setter) => {
					return (
						<Input type="textarea" value={x}
							setter={setter} disabled={disabled}
						/>
					)
				}}
			/>

			{children}

			<h3>Other</h3>

			<Input field="related" type="array"
				setter={setData} value={data} disabled={disabled}
				forEach={(x, i, setter) => {
					return (
						<Input value={x} setter={setter} disabled={disabled} />
					)
				}}
			/>

			<Input field="example" type="container"
				setter={setData} value={data}
				inputs={(x, setter) => {
					return (
						<Fragment>
							<Input field="code"
								value={x} setter={setter} disabled={disabled}
							/>
							<Input field="description" type="textarea"
								value={x} setter={setter} disabled={disabled}
							/>
						</Fragment>
					)
				}}
			/>
		</section>
	)
}
