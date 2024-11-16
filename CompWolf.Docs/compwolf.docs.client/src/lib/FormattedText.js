import { Fragment } from "react"
import { Reference, SimpleReference } from "./CodeComponents"

function BaseFormatter({ children, regexBegin, regexEnd, Formatter, RestFormatter }) {
	return children
		.split(new RegExp(regexBegin, `g`))
		.map((s, i) => {
			if (i == 0) return [s]
			const index = s.search(new RegExp(regexEnd, `g`))
			return (index == -1)
				? [s]
				: [s.substring(0, index), s.substring(index)]
		})
		.map((s, i) => {
			if (s[1] === undefined) return <RestFormatter key={i}>{s[0]}</RestFormatter>
			return (
				<Fragment key={i}>
					<Formatter>{s[0]}</Formatter><RestFormatter>{s[1]}</RestFormatter>
				</Fragment>
			)
		})
}
function ListFormatter({ children, NextFormatting, level = 0 }) {
	let organizedChildren = children
		.split(/(\r?\n)/)
		.reduce((output, line) => {
			var level = 0
			for (level = 0; level < line.length && line[level] === `-`; ++level)
				;

			var outputLevel = 0
			var targetOutput = output
			while (level > outputLevel) {
				if (targetOutput.lines.length === 0) {
					targetOutput.lines.push({
						level: outputLevel,
						text: "",
						lines: [],
					})
				} else if (targetOutput.lines[targetOutput.lines.length - 1].lines === undefined) {
					targetOutput.lines[targetOutput.lines.length - 1] = {
						level: outputLevel,
						text: targetOutput.lines[targetOutput.lines.length - 1],
						lines: [],
					}
				}
				targetOutput = targetOutput.lines[targetOutput.lines.length - 1]
				outputLevel += 1
			}

			targetOutput.lines.push(line.slice(level, line.length))
			return output

		}, {
			level: 0,
			text: "",
			lines: [],
		})

	function itemDrawer(target) {
		if (target.level === undefined) return <NextFormatting>{target}</NextFormatting>
		return (
			<Fragment>
				<NextFormatting>{target.text}</NextFormatting>
				<ul>
					{target.lines.map((line, i) => <li key={i}>{itemDrawer(line)}</li>)}
				</ul>
			</Fragment>
		)
	}

	return (
		<Fragment>
			{organizedChildren.lines.map((line, i) => {
				if (line.level === undefined) return <NextFormatting key={i}>{line}</NextFormatting>
				else return (
					<ul key={i}>
						{line.lines.map((line, i) => <li key={i}>{itemDrawer(line)}</li>)}
					</ul>
				)
			})}
		</Fragment>
	)
}
function ReferenceFormatter({ children, NextFormatting }) {
	return <BaseFormatter regexBegin="compwolf\:\:" regexEnd="[^\w_]" RestFormatter={NextFormatting} Formatter={({ children }) => <SimpleReference name={children} />}>
		{children}
	</BaseFormatter>
}
function StdReferenceFormatter({ children, NextFormatting }) {
	return <BaseFormatter regexBegin="std\:\:" regexEnd="[^\w_]" RestFormatter={NextFormatting} Formatter={({ children }) => <Reference path={`https://duckduckgo.com/?sites=cppreference.com&q=` + children}>std::{children}</Reference>}>
		{children}
	</BaseFormatter>
}

function CombinedFormatters({ children, Formatters }) {
	if (Formatters.length === 1) return Formatters[0]({
		children: children,
		NextFormatting: ({ children }) => children
	})
	else return Formatters[0]({
		children: children,
		NextFormatting: ({ children }) => CombinedFormatters({
			children: children,
			Formatters: Formatters.slice(1, Formatters.length)
		})
	})
}

export default function FormattedText({ children }) {
	if (children === undefined) return false
	return <CombinedFormatters Formatters={[ListFormatter, ReferenceFormatter, StdReferenceFormatter]}>{children}</CombinedFormatters>
}
