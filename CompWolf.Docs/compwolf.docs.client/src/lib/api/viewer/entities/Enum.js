import { Fragment } from "react"
import { Declaration } from "@/lib/CodeComponents"
import FormattedText from "@/lib/FormattedText"
import BaseEntityViewer from "./BaseEntity"

export default function EnumViewer({ data }) {
	const is_empty = (x) => { return x == undefined || x.length == 0 }

	return (
		<BaseEntityViewer data={data} top={
			<Fragment>
				<big id="Declaration">
					<Declaration>{data.declaration}</Declaration>
				</big>
			</Fragment>
		}>
			<section id="Values">
				<h2>Values</h2>
				<table className="memberTable">
					<thead>
						<tr>
							<th>Name</th>
							<th>Description</th>
						</tr>
					</thead>
					<tbody>
						{data.values?.map((x, i) => {
							return (
								<tr key={i}>
									<td>
										{x.name}
									</td>
									<td>
										<FormattedText>{x.briefDescription}</FormattedText>
									</td>
								</tr>
							)
						})}
					</tbody>
				</table>
			</section>
		</BaseEntityViewer>
	)
}