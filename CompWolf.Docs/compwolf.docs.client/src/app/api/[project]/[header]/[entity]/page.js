import EntityViewer from "@/lib/api/viewer/Entity"
import { getOverview, getEntity } from "@/lib/api/getEntity"
import betterEncodeURIComponent from "@/lib/betterEncodeURIComponent"

export default async function EntityPage({ params }) {
    if (params.project === "%5Bproject%5D") return <div />
    let project = decodeURIComponent(params.project)
    let header = decodeURIComponent(params.header)
    let entity = decodeURIComponent(params.entity)

    const data = await getEntity(project, header, entity)

    return <EntityViewer data={data} />
}

export async function generateStaticParams() {
    const overview = await getOverview()

    return overview.projects.map(project =>
        project.headers.map(header =>
            header.entities.map(entity => {
                return {
                    project: betterEncodeURIComponent(project.name),
                    header: betterEncodeURIComponent(header.name),
                    entity: betterEncodeURIComponent(entity.name),
                }
            })
        )
    ).flat(Infinity)
}
