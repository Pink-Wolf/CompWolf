import { Reference } from "@/lib/CodeComponents"
import FormattedText from "@/lib/FormattedText"

export default function ProjectListViewer({ data }) {
	const is_empty = (x) => { return x == undefined || x.length == 0 }

	return (
		<section>
			<h2>Projects:</h2>
			<ul>
				{data.map((x, i) => {
					return (
						<li key={i}>
							<Reference path={`/api/${x.name}`}>{x.name}</Reference>
						</li>
					)
				})}
			</ul>
		</section>
	)
}