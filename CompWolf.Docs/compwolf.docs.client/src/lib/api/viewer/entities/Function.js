import { Declaration } from "@/lib/CodeComponents"
import FormattedText from "@/lib/FormattedText"
import BaseEntityViewer from "./BaseEntity"
import { Fragment } from "react"

export default function FunctionViewer({ data }) {
	const parameters = Object.entries(data.parameterDescriptions)

	return (
		<BaseEntityViewer data={data}>

			<section hidden={parameters.length === 0 && !data.returnDescription}>
				<h2>Parameters</h2>
				<table className="memberTable">
					<thead>
						<tr>
							<th>Name</th>
							<th>Description</th>
						</tr>
					</thead>
					<tbody>
						<tr hidden={!data.returnDescription}>
							<td>[Returns]</td>
							<td>
								<FormattedText>{data.returnDescription}</FormattedText>
							</td>
						</tr>
						{parameters.map((x, i) => {
							return (
								<tr key={i}>
									<td> {x[0]} </td>
									<td>
										<FormattedText>{x[1]}</FormattedText>
									</td>
								</tr>
							)
						})}
					</tbody>
				</table>
			</section>

			<section hidden={!data.throwDescription} id="Exceptions">
				<h2>Exceptions</h2>
				{ data.throwDescription }
			</section>
		</BaseEntityViewer>
	)
}