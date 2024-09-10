import { Fragment, useState } from "react"
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

	const [visibleConstructor, setVisibleConstructor] = useState(false)
	const [visibleMember, setVisibleMember] = useState([])

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
			<h3>Template Parameters</h3>
			<Input field="templateParameters" label="" type="array"
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

		</Fragment>,
		children: <Fragment>

			<Input field="constructor" type="container"
				setter={setData} value={data} disabled={disabled}
				inputs={(x, setter) => {
					return (
						<Fragment>
							<Input type="boolCheckbox" label="Visible:"
								value={visibleConstructor} setter={setVisibleConstructor} />
							{!visibleConstructor ? null : <Fragment>
								<Input field="copyable" type="boolCheckbox"
									value={data} setter={setData} disabled={disabled} />
								<Input field="movable" type="boolCheckbox"
									value={data} setter={setData} disabled={disabled} />

								<FunctionEditor data={x} setData={setter}
									disabled={disabled} immutableName />
							</Fragment>}
						</Fragment>
					)
				}}
			/>

			<Input field="memberGroups" type="array"
				setter={setData} value={data} disabled={disabled}
				forEach={(x, groupI, setter) => {
					return (
						<Fragment>
							<Input field="name"
								value={x} setter={setter} disabled={disabled}
							/>
							<Input field="items" type="array"
								setter={setter} value={x} disabled={disabled}
								forEach={(x, itemI, setter) => {
									const fieldIndex = `${groupI}.${itemI}`
									return (
										<Fragment>
											<h1>{x.name}</h1>
											<Input type="boolCheckbox" field={fieldIndex} label="Details:"
												value={visibleMember} setter={setVisibleMember} />
											{!visibleMember[fieldIndex] ? null : <Fragment>
												<MutableTypeEditor data={x} setData={setter}
													disabled={disabled} />
											</Fragment>}
										</Fragment>
									)
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