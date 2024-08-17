import { Fragment } from "react"
import Input from "@/lib/Input"
import BaseEntityEditor from "./BaseEntity"

export function toDeclarationType(data) {
	return {
		declaration: "",
		...data
	}
}

export default function DeclarationEditor(params) {
	const data = params.data
	const setData = params.setData
	const disabled = params.disabled

	return BaseEntityEditor({
		...params,
		top: <Fragment>
			{params.top}
			<Input field="declaration"
				setter={setData} value={data} disabled={disabled}
			/>
		</Fragment>
	})
}