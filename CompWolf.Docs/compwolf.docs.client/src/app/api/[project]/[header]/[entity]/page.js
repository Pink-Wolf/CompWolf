import EntityViewer from "@/lib/api/viewer/Entity"
import { getOverview, getEntity } from "@/lib/api/getEntity"
import { generateStaticParamsEncoder, decodeStaticParamsEncoder } from "@/lib/betterEncodeURIComponent"

export default async function EntityPage(input) {
    let params = await input.params
    if (params.project === "%5Bproject%5D") return <div />
    let project = decodeStaticParamsEncoder(params.project)
    let header = decodeStaticParamsEncoder(params.header)
    let entity = decodeStaticParamsEncoder(params.entity)

    const data = await getEntity(project, header, entity)

    return <EntityViewer data={data} />
}

export async function generateMetadata({params}) {
    return {
        title: `${(await params).entity}`,
    }
}

export async function generateStaticParams() {
    const overview = await getOverview()

    return overview.projects.map(project =>
        project.headers.map(header =>
            header.entities.map(entity => {
                return {
                    project: generateStaticParamsEncoder(project.name),
                    header: generateStaticParamsEncoder(header.name),
                    entity: generateStaticParamsEncoder(entity.name),
                }
            })
        )
    ).flat(Infinity)
}
