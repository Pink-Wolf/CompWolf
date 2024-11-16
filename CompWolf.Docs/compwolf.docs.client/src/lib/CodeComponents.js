import Link from "next/link"
import SyntaxHighlighter from "react-syntax-highlighter/dist/esm/default-highlight"
import { xcode } from "react-syntax-highlighter/dist/esm/styles/hljs"
import { getPathTo } from "./api/getEntity"
import betterEncodeURIComponent from "@/lib/betterEncodeURIComponent"

export function Reference({ path, children }) {
	return (
		path
			? <Link href={path}>
				<code>
					{children}
				</code>
			</Link>
			: <code>
				{children}
			</code>
	)
}
export async function SimpleReference({ target, name }) {
	if (typeof (target) !== typeof (undefined)) {
		const pathArray = [`api`]

		if (target.project !== target) {
			pathArray.push(target.project.name)

			if (target.header !== target) {
				pathArray.push(target.header.name)

				for (var i = 0; i < target.owners.length; i++) {
					pathArray.push(target.owners[i].name)
				}
			}
		}

		pathArray.push(target.name)

		const finalPath = '/' + pathArray.map(betterEncodeURIComponent).join('/')
		return <Reference path={finalPath}>{target.name}</Reference>
	}

	const finalPath = await getPathTo(name)
	return <Reference path={finalPath}>{name}</Reference>
}

export function CodeViewer({ children }) {
	return (
		<SyntaxHighlighter language="cpp" style={xcode}>
			{children}
		</SyntaxHighlighter>
	)
}
export function Declaration({ children, ...props }) {

	return (
		<big className="declaration" {...props}>
			<CodeViewer>
				{children}
			</CodeViewer>
		</big>
	)
}