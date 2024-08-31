import { Declaration } from "@/lib/CodeComponents"
import FormattedText from "@/lib/FormattedText"
import BaseEntityViewer from "./BaseEntity"
import { Fragment } from "react"

export default function DeclarationEntityViewer(params) {
	const is_empty = (x) => { return x == undefined || x.length == 0 }

	return BaseEntityViewer({
		...params,
		top: <Fragment>
			{params.top}
			<Declaration id="Declaration">{params.data.declaration}</Declaration>
		</Fragment>
	})
}