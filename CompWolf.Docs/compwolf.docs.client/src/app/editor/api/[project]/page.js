import { getProject, getOverview } from "@/lib/api/getEntity"
import ProjectEditor from "@/lib/api/editor/Project"
import betterEncodeURIComponent from "@/lib/betterEncodeURIComponent"

export default async function ProjectEditorPage({ params }) {
    if (params.project === "%5Bproject%5D") return <div />
    let project = decodeURIComponent(params.project)

    const overview = await getOverview()
    const data = {
        ...(await getProject(project)),
        headers: overview
            .projects.find((x) => { return x.name === project })
            .headers
    }

    return <ProjectEditor data={data} />
}

import { generateStaticParams as _generateStaticParams } from "@/app/api/[project]/page"

export async function generateStaticParams() {
    if (process.env.INCLUDE_API_EDITOR != "1") return [{ project: "[project]" }]
    return _generateStaticParams()
}
