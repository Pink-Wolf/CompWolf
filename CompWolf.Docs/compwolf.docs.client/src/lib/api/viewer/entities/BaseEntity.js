import Style from "@/styles/EntityViewer.css"

import { CodeViewer, Reference, SimpleReference } from "@/lib/CodeComponents"
import FormattedText from "@/lib/FormattedText"
import betterEncodeURIComponent from "@/lib/betterEncodeURIComponent"

export default function BaseEntityViewer({ data, top, children }) {
	const is_empty = (x) => { return x == undefined || x.length == 0 }

	return (
		<section>
			<h1 id="Path">
				{[data.namespace, ...data.owners, data.name].join("::")}
			</h1>
			<small>
				<p id="Project">In project <Reference path={`/api/${data.project}`}>{data.project}</Reference></p>
				<p id="Header">Defined in header <Reference path={`/api/${data.project}/${data.header}`}>&lt;{data.header}&gt;</Reference></p>
				<p id="Owner" hidden={!data.owners.length}>Member of <Reference path={`/api/${data.project}/${data.header}/${data.owners.map(x => betterEncodeURIComponent(x)).join("/")}`}>{data.owners.join("::")}</Reference></p>
			</small>

			{top}

			<div id="Description">
				{data.detailedDescription.map((x, i) => {
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