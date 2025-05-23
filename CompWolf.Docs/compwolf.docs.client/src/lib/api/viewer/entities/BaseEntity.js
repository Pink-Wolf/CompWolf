import Style from "@/styles/EntityViewer.css"

import { CodeViewer, Declaration, SimpleReference } from "@/lib/CodeComponents"
import FormattedText from "@/lib/FormattedText"
import betterEncodeURIComponent from "@/lib/betterEncodeURIComponent"

export default function BaseEntityViewer({ data, top, children }) {
	const is_empty = (x) => { return x == undefined || x.length == 0 }

	return (
		<section>
			<h1 id="Path">
				{[data.namespace, ...data.owners.map(x => x.name), data.name].join("::")}
			</h1>
			<small>
				<p id="Project">In project <SimpleReference target={data.project} /></p>
				<p id="Header">Defined in header <SimpleReference target={data.header} /></p>
				<p id="Owner" hidden={!data.owners.length}>Member of {data.owners.map((x, i) => <SimpleReference key={i} target={x} />)}</p>
			</small>

			{top}

			<div id="Declarations" type="1" hidden={data.declarations.length !== 1}>
				<Declaration>{data.declarations[0].declaration}</Declaration>
				<FormattedText>{data.declarations[0].description}</FormattedText>
			</div>
			<ol id="Declarations" type="1" hidden={data.declarations.length === 1}>
				{data.declarations.map((x, i) => {
					return (
						<li key={i + 1}>
							<FormattedText>{x.description}</FormattedText>
							<Declaration>{x.declaration}</Declaration>
						</li>
					)
				})}
				{data.name !== data.owners[data.owners.length - 1]}
			</ol>

			<div id="Description">
				{data.descriptions.map((x, i) => {
					return (
						<section key={i}>
							<FormattedText>{x}</FormattedText>
						</section>
					)
				})}
			</div>

			<section hidden={is_empty(data.warnings)} className="warning" id="Warnings">
				<h2>Warning</h2>
				{data.warnings?.map((x, i) => {
					return <blockquote key={i}><FormattedText>{x}</FormattedText></blockquote>
				})}
			</section>

			<section hidden={Object.entries(data.templateParameterDescriptions).length === 0}>
				<h2>Template Parameters</h2>
				<table className="memberTable">
					<thead>
						<tr>
							<th>Name</th>
							<th>Description</th>
						</tr>
					</thead>
					<tbody>
						{Object.entries(data.templateParameterDescriptions).map((x, i) => {
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

			{children}

			<section hidden={!data.example || !data.example.name} id="Example">
				<h2>Example</h2>
				<div>
					<FormattedText>{data.example?.description}</FormattedText>
				</div>
				<CodeViewer>
					{data.example?.code}
				</CodeViewer>
			</section>

			<section hidden={is_empty(data.related)} id="Related">
				<h2>Related</h2>
				<ul>
					{data.related?.map((x, i) => {
						return (
							<li key={i}>
								{<SimpleReference name={x} />}
							</li>
						)
					})}
				</ul>
			</section>

		</section>
	)
}