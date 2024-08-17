import { Fragment } from "react"
import Input from "@/lib/Input"
import BaseEntityEditor from "./BaseEntity"

export function toFunctionType(data) {
	return {
		overloads: [],
		exceptions: [],
		...data,
	}
}

export default function FunctionEditor(parameters) {
	const data = parameters.data
	const setData = parameters.setData
	const disabled = parameters.disabled

	return BaseEntityEditor({
		...parameters,
		top: <Fragment>
			<Input field="overloads" type="array"
				setter={setData} value={data} disabled={disabled}
				forEach={(x, i, setter) => {
					return (
						<Fragment>
							<Input field="declaration"
								value={x} setter={setter} disabled={disabled}
							/>
							<Input field="description" type="textarea"
								value={x} setter={setter} disabled={disabled}
							/>
						</Fragment>
					)
				}}
			/>
		</Fragment>,
		children: <Fragment>
			<Input field="exceptions" type="array"
				setter={setData} value={data} disabled={disabled}
				forEach={(x, i, setter) => {
					return (
						<Input type="textarea"
							value={x} setter={setter} disabled={disabled}
						/>
					)
				}}
			/>
		</Fragment>
	})
}