import Link from "next/link"
import SyntaxHighlighter from "react-syntax-highlighter/dist/esm/default-highlight"
import { xcode } from "react-syntax-highlighter/dist/esm/styles/hljs"
import { getPathTo, getPathToObject, getFullNameOfObject, getShortNameOfObject } from "./api/getEntity"
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
export async function SimpleReference({ target, name, useShortName }) {
	const inputType = typeof (target) !== typeof (undefined)
		? `target`
		: `name`

	const finalPath = (inputType === `target`)
		? getPathToObject(target)
		: await getPathTo(name)

	const finalName = (inputType === `target`)
		? useShortName ? getShortNameOfObject(target) : getFullNameOfObject(target)
		: name

	return <Reference path={finalPath}>{finalName}</Reference>
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