import Link from "next/link"
import SyntaxHighlighter from "react-syntax-highlighter/dist/esm/default-highlight"
import { xcode } from "react-syntax-highlighter/dist/esm/styles/hljs"
import { getPathTo } from "./api/getEntity"

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
export async function SimpleReference({ name }) {
	const path = await getPathTo(name)
	return <Reference path={path}>{name}</Reference>
}

export function CodeViewer({ children }) {
	return (
		<SyntaxHighlighter language="cpp" style={xcode}>
			{children}
		</SyntaxHighlighter>
	)
}
export function Declaration({ children }) {
	return (
		<big className="declaration">
			<CodeViewer>
				{children}
			</CodeViewer>
		</big>
	)
}