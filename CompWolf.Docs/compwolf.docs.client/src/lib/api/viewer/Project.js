import { SimpleReference } from "@/lib/CodeComponents"
import FormattedText from "@/lib/FormattedText"

export default function ProjectViewer({ data }) {
	return (
		<section>
			<h1 id="Declaration">
				{`${data.name}`}
			</h1>
			<h2>Header-files:</h2>
			<ul>
				{data.headers.map((x, i) => {
					return (
						<li key={i}>
							<SimpleReference target={x} />: <FormattedText>{x.descriptions[0]}</FormattedText>
						</li>
					)
				})}
			</ul>
		</section>
	)
}