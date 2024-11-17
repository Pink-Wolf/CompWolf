import { Fragment } from "react"
import { Declaration, Reference, SimpleReference } from "@/lib/CodeComponents"
import FormattedText from "@/lib/FormattedText"
import betterEncodeURIComponent from "@/lib/betterEncodeURIComponent"
import BaseEntityViewer from "./BaseEntity"

export default function ClassViewer({ data }) {
	const is_empty = (x) => { return x == undefined || x.length == 0 }

	return (
		<BaseEntityViewer data={data} top={
			<Fragment>
				<small hidden={is_empty(data.baseClasses)}>
					Inherits from: {
						is_empty(data.baseClasses) ? "nothing." : <ul>
							{data.baseClasses.map((x, i) => {
								return (
									<li key={i}>
										<SimpleReference name={x} />
									</li>
								)
							})}
						</ul>
					}
				</small>
			</Fragment>
		}>
			<section hidden={is_empty(data.templateParameters)}>
				<h2>Template Parameters</h2>
				<table className="memberTable">
					<thead>
						<tr>
							<th>Name</th>
							<th>Description</th>
						</tr>
					</thead>
					<tbody>
						{data.templateParameters?.map((x, i) => {
							return (
								<tr key={i}>
									<td> {x.name} </td>
									<td>
										<FormattedText>{x.description}</FormattedText>
									</td>
								</tr>
							)
						})}
					</tbody>
				</table>
			</section>
			
			<section hidden={is_empty(data.members)} id="MemberFunctions">
				<h2>Members</h2>
				<table className="memberTable">
					<thead>
						<tr>
							<th>Name</th>
							<th>Description</th>
						</tr>
					</thead>
					<tbody>
						{Object.entries(data.members ?? {}).map((x, i) => {
							return (
								<Fragment key={i}>
									<tr hidden={is_empty(x[0])}>
										<td colSpan="2">
											<h3>{x[0]}</h3>
										</td>
									</tr>
									{x[1].map((x, i) => {
										return (
											<tr key={i}>
												<td>
													<SimpleReference target={x} useShortName={true} />
												</td>
												<td>
													<FormattedText>{x.descriptions[0]}</FormattedText>
												</td>
											</tr>
										)
									})}
								</Fragment>
							)
						})}
					</tbody>
				</table>
			</section>
		</BaseEntityViewer>
	)
}