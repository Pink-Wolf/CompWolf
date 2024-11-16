import { Declaration, Reference, SimpleReference } from "@/lib/CodeComponents"
import FormattedText from "@/lib/FormattedText"

export default function HeaderViewer({ data }) {
	return (
		<section>
			<small>
				<p id="Project">In project <SimpleReference target={data.project} /></p>
			</small>
			<h1 id="Declaration">
				<Declaration>{`<${data.name}>`}</Declaration>
			</h1>
			<section id="Description">
				{data.descriptions.map((x, i) => {
					return (
						<section key={i}>
							<FormattedText>{x}</FormattedText>
						</section>
					)
				})}
			</section>
			<h2>Entities:</h2>
			<ul>
				{data.entities.map((x, i) => {
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