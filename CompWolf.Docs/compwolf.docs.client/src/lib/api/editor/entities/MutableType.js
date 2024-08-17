'use client'

import { Fragment } from "react"
import ClassEditor, { toClassType, toConceptType } from "./Class"
import DeclarationEditor, { toDeclarationType } from "./DeclarationEntity"
import EnumEditor, { toEnumType } from "./Enum"
import FunctionEditor, { toFunctionType } from "./Function"
import Input from "@/lib/Input"

export function toMutableTypeType(data) {
	return {
		type: "undefined",
		...data
	}
}

export default function MutableTypeEditor(params) {
	const {
		data: data, setData: setData,
		disabled: disabled
	} = params
	const typeOptions = params.typeOptions ?? [
		"undefined",
		"concept",
		"class",
		"function",
		"variable",
		"enum",
		"alias",
		"macro",
	]

	function setType(newData) {
		switch (newData.type) {
			case 'concept': setData(toConceptType(newData)); break
			case `class`: setData(toClassType(newData)); break
			case `function`: setData(toFunctionType(newData)); break
			case `enum`: setData(toEnumType(newData)); break
			default: setData(toDeclarationType(newData)); break
		}
	}

	var editor = DeclarationEditor
	switch (data?.type) {
		case 'concept':
		case `class`: editor = ClassEditor; break
		case `function`: editor = FunctionEditor; break
		case `enum`: editor = EnumEditor; break
		default: break
	}

	return (
		<Fragment>
			<Input field="type" type="select"
				setter={setType} value={data}
				disabled={disabled} options={typeOptions}
			/>
			{editor(params)}
		</Fragment>
	)
}
