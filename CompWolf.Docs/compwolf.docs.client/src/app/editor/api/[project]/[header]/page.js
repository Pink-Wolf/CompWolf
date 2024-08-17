import { getHeader, getOverview } from "@/lib/api/getEntity"
import HeaderEditor from "@/lib/api/editor/Header"
import betterEncodeURIComponent from "@/lib/betterEncodeURIComponent"

export default async function HeaderEditorPage({ params }) {
    if (params.project === "%5Bproject%5D") return <div />
    let project = decodeURIComponent(params.project)
    let header = decodeURIComponent(params.header)

    const overview = await getOverview()
    const data = {
        ...(await getHeader(project, header)),
        entities: overview
            .projects.find((x) => { return x.name === project })
            .headers.find((x) => { return x.name === header })
            .entities
    }

    return <HeaderEditor data={data} />
}

import { generateStaticParams as _generateStaticParams } from "@/app/api/[project]/[header]/page"

export async function generateStaticParams() {
    if (process.env.INCLUDE_API_EDITOR != "1") return [{ project: "[project]", header: "[header]" }]
    return _generateStaticParams()
}
