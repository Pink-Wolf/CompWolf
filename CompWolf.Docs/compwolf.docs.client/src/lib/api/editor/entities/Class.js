import { Fragment } from "react"
import Input from "@/lib/Input"
import DeclarationEditor, { toDeclarationType } from "./DeclarationEntity"
import FunctionEditor, { toFunctionType } from "./Function"
import MutableTypeEditor, { toMutableTypeType } from "./MutableType"
import { defaultBaseEntity } from "./BaseEntity"

export function toClassType(data) {
	return {
		baseClasses: [],
		templateParameters: [],
		copyable: true,
		movable: true,
		constructor: toFunctionType(defaultBaseEntity),
		memberGroups: [],
		...toDeclarationType(data)
	}
}

export function toConceptType(data) {
	return toClassType(data)
}

export default function ClassEditor(parameters) {
	const data = parameters.data
	const setData = parameters.setData
	const disabled = parameters.disabled

	return DeclarationEditor({
		...parameters,
		top: <Fragment>
			<Input field="baseClasses" type="array"
				setter={setData} value={data} disabled={disabled}
				forEach={(x, i, setter) => {
					return (
						<Input value={x} setter={setter} disabled={disabled} />
					)
				}}
			/>

		</Fragment>,
		children: <Fragment>
			<Input field="templateParameters" type="array"
				setter={setData} value={data} disabled={disabled}
				forEach={(x, i, setter) => {
					return (
						<Fragment>
							<Input field="name"
								value={x} setter={setter} disabled={disabled}
							/>
							<Input field="description" type="textarea"
								value={x} setter={setter} disabled={disabled}
							/>
						</Fragment>
					)
				}}
			/>

			<Input field="constructor" type="container"
				setter={setData} value={data} disabled={disabled}
				inputs={(x, setter) => {
					return (
						<Fragment>
							<Input field="copyable" type="boolCheckbox"
								value={data} setter={setData} disabled={disabled} />
							<Input field="movable" type="boolCheckbox"
								value={data} setter={setData} disabled={disabled} />

							<FunctionEditor data={x} setData={setter}
								disabled={disabled} immutableName />
						</Fragment>
					)
				}}
			/>

			<Input field="memberGroups" type="array"
				setter={setData} value={data} disabled={disabled}
				forEach={(x, i, setter) => {
					return (
						<Fragment>
							<Input field="name"
								value={x} setter={setter} disabled={disabled}
							/>
							<Input field="items" type="array"
								setter={setter} value={x} disabled={disabled}
								forEach={(x, i, setter) => {
									return MutableTypeEditor({
										data: x, setData: setter,
										disabled: disabled
									})
								}}
								newValue={() => {
									return toMutableTypeType({
										...defaultBaseEntity,
										name: ""
									})
								}}
							/>
						</Fragment>
					)
				}}
				newValue={() => {
					return {
						name: "",
						items: []
					}
				}}
			/>
		</Fragment>
	})
}