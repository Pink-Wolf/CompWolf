import { Reference } from "@/lib/CodeComponents"
import FormattedText from "@/lib/FormattedText"

export default function ProjectViewer({ data }) {
	const is_empty = (x) => { return x == undefined || x.length == 0 }
	return (
		<section>
			<h1 id="Declaration">
				{`${data.project}`}
			</h1>
			<section id="Description">
				{data.detailedDescription.map((x, i) => {
					return (
						<section key={i}>
						<FormattedText>{x}</FormattedText>
						</section>
					)
				})}
			</section>
			<h2>Header-files:</h2>
			<ul>
				{data.headers.map((x, i) => {
					return (
						<li key={i}>
							<Reference path={`/api/${data.project}/${x.name}`}>{x.name}</Reference>: <FormattedText>{x.briefDescription}</FormattedText>
						</li>
					)
				})}
			</ul>
		</section>
	)
}