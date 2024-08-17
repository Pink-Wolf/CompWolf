import { getOverview } from "@/lib/api/getEntity"
import ProjectListEditor from "@/lib/api/editor/ProjectList"

export default async function ApiPage({ }) {
    const overview = await getOverview()

    return <ProjectListEditor data={overview} />
}
