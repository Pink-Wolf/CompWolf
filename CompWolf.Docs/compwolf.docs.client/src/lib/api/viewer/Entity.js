import ClassViewer from "./entities/Class"
import DeclarationEntityViewer from "./entities/DeclarationEntity"
import EnumViewer from "./entities/Enum"
import FunctionViewer from "./entities/Function"

export default function EntityViewer({ data }) {
	switch (data?.type) {
		case 'concept':
		case `class`: return <ClassViewer data={data} />
		case `function`: return <FunctionViewer data={data} />
		case `enum`: return <EnumViewer data={data} />
		default: return <DeclarationEntityViewer data={data} />
    }
}