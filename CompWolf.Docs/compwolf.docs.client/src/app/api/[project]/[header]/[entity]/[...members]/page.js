import { getOverview, getEntity } from "@/lib/api/getEntity"
import EntityViewer from "@/lib/api/viewer/Entity"
import { generateStaticParamsEncoder, decodeStaticParamsEncoder } from "@/lib/betterEncodeURIComponent"

export default async function MemberPage(input) {
    let params = await input.params
    if (params.project === "%5Bproject%5D") return <div />
    let project = decodeStaticParamsEncoder(params.project)
    let header = decodeStaticParamsEncoder(params.header)
    let entity = decodeStaticParamsEncoder(params.entity)
    let members = params.members.map(x => decodeStaticParamsEncoder(x))

    const entities = [await getEntity(project, header, entity)]

    for (const i in members) {
        let entityI = Number(i)
        let targetName = members[entityI]
        let owner = entities[entities.length - 1]

        const nextEntity = Object.values(owner.members).flat().find(x => x.name == targetName)
        entities.push(nextEntity)
    }

    let data = entities[entities.length - 1]

    return <EntityViewer data={data} />
}

function getMembers(entity, members) {
    return Object.values(members).flat().map(member => {
        let data = {
            ...entity,
            members: [...entity.members, generateStaticParamsEncoder(member.name)]
        }
        return [
            data,
            ...getMembers(data, member.members)
        ]
    })
}

export async function generateMetadata({ params }) {
    let { members } = await params
    return {
        title: `${members[members.length-1]}`,
    }
}

export async function generateStaticParams() {
    const overview = await getOverview()

    return overview.projects.map(project =>
        project.headers.map(header =>
            header.entities.map(entity =>
                getMembers({
                    project: generateStaticParamsEncoder(project.name),
                    header: generateStaticParamsEncoder(header.name),
                    entity: generateStaticParamsEncoder(entity.name),
                    members: []
                }, entity.members)
            )
        )
    ).flat(Infinity)
}
