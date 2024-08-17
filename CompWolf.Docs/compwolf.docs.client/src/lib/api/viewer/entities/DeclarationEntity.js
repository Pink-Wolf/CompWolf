import { Declaration } from "@/lib/CodeComponents"
import FormattedText from "@/lib/FormattedText"
import BaseEntityViewer from "./BaseEntity"
import { Fragment } from "react"

export default function VariableViewer(params) {

	return BaseEntityViewer({
		...params,
		top: <Fragment>
			<p id="Declaration">
				<Declaration>{params.data.declaration}</Declaration>
			</p>
			{params.top}
		</Fragment>
	})
}