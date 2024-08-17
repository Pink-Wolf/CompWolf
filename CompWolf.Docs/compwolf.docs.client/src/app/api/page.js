import { getOverview } from "@/lib/api/getEntity"
import ProjectListViewer from "@/lib/api/viewer/ProjectList"

export default async function ApiPage({ }) {
    const overview = await getOverview()

    return <ProjectListViewer data={overview} />
}
