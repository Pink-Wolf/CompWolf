import { Fragment } from "react"
import { Declaration, Reference, SimpleReference } from "@/lib/CodeComponents"
import FormattedText from "@/lib/FormattedText"
import betterEncodeURIComponent from "@/lib/betterEncodeURIComponent"
import DeclarationEntityViewer from "./DeclarationEntity"

export default function ClassViewer({ data }) {
	const is_empty = (x) => { return x == undefined || x.length == 0 }

	const dataPath = `/api/
		${data.project}/
		${data.header}/
		${(data.owners ?? []).map(x => `${betterEncodeURIComponent(x)}/`).join()}
		${betterEncodeURIComponent(data.name)}`
	function MemberReference({ name }) {
		return (
			<Reference path={`${dataPath}/${betterEncodeURIComponent(name)}`}>
				{name}
			</Reference>
		)
	}

	return (
		<DeclarationEntityViewer data={data} top={
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
			
			<section hidden={is_empty(data.memberGroups) && is_empty(data.constructor?.overloads)} id="MemberFunctions">
				<h2>Members</h2>
				<table className="memberTable">
					<thead>
						<tr>
							<th>Name</th>
							<th>Description</th>
						</tr>
					</thead>
					<tbody>
						<tr hidden={is_empty(data.constructor?.overloads)}>
							<td>
								<Reference path={`${dataPath}/${betterEncodeURIComponent(data.name)}`}>
									(constructors)
								</Reference>
							</td>
							<td>
								<FormattedText>{data.constructor?.briefDescription}</FormattedText>
							</td>
						</tr>
						{data.memberGroups?.map((x, i) => {
							return (
								<Fragment key={i}>
									<tr hidden={is_empty(data.baseClasses)}>
										<td colSpan="2" hidden={is_empty(x.name) && is_empty(x.description)}>
											<h3>{x.name}</h3>
											<div>
												<FormattedText>{x.description}</FormattedText>
											</div>
										</td>
									</tr>
									{x.items.map((x, i) => {
										return (
											<tr key={i}>
												<td>
													<MemberReference name={x.name} />
												</td>
												<td>
													<FormattedText>{x.briefDescription}</FormattedText>
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
		</DeclarationEntityViewer>
	)
}