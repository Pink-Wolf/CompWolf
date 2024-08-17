import EntityEditor from "@/lib/api/editor/Entity"
import { getOverview, getEntityRaw } from "@/lib/api/getEntity"
import betterEncodeURIComponent from "@/lib/betterEncodeURIComponent"

export default async function EntityEditorPage({ params }) {
    if (params.project === "%5Bproject%5D") return <div />
    let project = decodeURIComponent(params.project)
    let header = decodeURIComponent(params.header)
    let entity = decodeURIComponent(params.entity)

    const data = await getEntityRaw(project, header, entity)

    return <EntityEditor data={data} />
}

import { generateStaticParams as _generateStaticParams } from "@/app/api/[project]/[header]/[entity]/page"

export async function generateStaticParams() {
    if (process.env.INCLUDE_API_EDITOR != "1") return [{ project: "[project]", header: "[header]", entity: "[entity]" }]
    return _generateStaticParams()
}
