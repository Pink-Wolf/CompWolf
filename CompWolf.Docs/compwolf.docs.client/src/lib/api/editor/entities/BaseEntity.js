'use client'

import Style from "@/styles/EntityViewer.css"

import { Fragment, useState } from "react"
import { useRouter } from "next/navigation"
import { postHeader } from "@/lib/api/getEntity"
import Input, { SubmitInput } from "@/lib/Input"
import { Reference } from "@/lib/CodeComponents"
import FormattedText from "@/lib/FormattedText"
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
			{top}

			{
				immutableName ? null : <Input field="name"
					setter={setData} value={data} disabled={disabled}
				/>
			}

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

			<Input field="warnings" type="array"
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

			<Input field="related" type="array"
				setter={setData} value={data} disabled={disabled}
				forEach={(x, i, setter) => {
					return (
						<Input value={x} setter={setter} disabled={disabled} />
					)
				}}
			/>
		</section>
	)
}
