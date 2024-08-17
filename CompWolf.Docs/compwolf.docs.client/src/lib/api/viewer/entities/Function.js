import { Declaration } from "@/lib/CodeComponents"
import FormattedText from "@/lib/FormattedText"
import BaseEntityViewer from "./BaseEntity"
import { Fragment } from "react"

export default function FunctionViewer({ data }) {
	const is_empty = (x) => { return x == undefined || x.length == 0 }
	
	return (
		<BaseEntityViewer data={data} top={
			<Fragment>
				<ol id="Declarations" type="1">
					{data.overloads.map((x, i) => {
						return (
							<li key={i + 1}>
								<Declaration>{x.declaration}</Declaration>
								<FormattedText>{x.description}</FormattedText>
							</li>
						)
					})}
					{data.name !== data.owners[data.owners.length - 1]}
				</ol>
			</Fragment>
		}>

			<section hidden={is_empty(data.exceptions)} id="Exceptions">
				<h2>Exceptions</h2>
				{data.exceptions?.map((x, i) => {
					return <blockquote key={i}><FormattedText>{x}</FormattedText></blockquote>
				})}
			</section>
		</BaseEntityViewer>
	)
}