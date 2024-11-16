import ClassViewer from "./entities/Class"
import BaseEntityViewer from "./entities/BaseEntity"
import EnumViewer from "./entities/Enum"
import FunctionViewer from "./entities/Function"

export default function EntityViewer({ data }) {
	switch (data?.type) {
		case 'Concept':
		case `Class`: return <ClassViewer data={data} />
		case `Function`: return <FunctionViewer data={data} />
		case `Enum`: return <EnumViewer data={data} />
		default: return <BaseEntityViewer data={data} />
    }
}