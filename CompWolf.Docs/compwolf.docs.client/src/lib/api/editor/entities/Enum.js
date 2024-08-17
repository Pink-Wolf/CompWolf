import { Fragment } from "react"
import Input from "@/lib/Input"
import DeclarationEditor, { toDeclarationType } from "./DeclarationEntity"

export function toEnumType(data) {
	return {
		values: [],
		...toDeclarationType(data)
	}
}

export default function EnumEditor(parameters) {
	const data = parameters.data
	const setData = parameters.setData
	const disabled = parameters.disabled

	return DeclarationEditor({
		...parameters,
		children: <Fragment>
			<Input field="values" type="array"
				setter={setData} value={data} disabled={disabled}
				forEach={(x, i, setter) => {
					return (
						<Fragment>
							<Input field="name"
								value={x} setter={setter} disabled={disabled}
							/>
							<Input field="briefDescription" type="textarea"
								value={x} setter={setter} disabled={disabled}
							/>
						</Fragment>
					)
				}}
			/>
		</Fragment>
	})
}