import Style from "../styles/MenuTree.css"
import { Fragment } from "react"

export default function MenuTree({ items, Display, pathPrefix }) {
	pathPrefix = pathPrefix ?? ""

	return (
		<ul className="menuTree">
			{items.map((x, i) => {
				let CurrentDisplay = x.Display ?? Display ?? ((x) => <Fragment/>)
				let renderer = (displayData, items) => {
					if (items === undefined) return CurrentDisplay(displayData)
					return (
						<Fragment>
							{CurrentDisplay(displayData)}
							{MenuTree({
								items: items,
								Display: CurrentDisplay,
								pathPrefix: `${pathPrefix}${displayData.children}/`,
							})}
						</Fragment>
					)
				}
				return (
					<li key={i}>
						{renderer({
							children: x.name,
							path: `${pathPrefix}${x.name}`,
						}, x.items)}

					</li>
				)
			})}
		</ul>
	)
}
