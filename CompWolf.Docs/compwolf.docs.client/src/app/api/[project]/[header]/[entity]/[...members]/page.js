import { getOverview, getEntity } from "@/lib/api/getEntity"
import EntityViewer from "@/lib/api/viewer/Entity"
import betterEncodeURIComponent from "@/lib/betterEncodeURIComponent"

export default async function MemberPage({ params }) {
    if (params.project === "%5Bproject%5D") return <div />
    let project = decodeURIComponent(params.project)
    let header = decodeURIComponent(params.header)
    let entity = decodeURIComponent(params.entity)
    let members = params.members.map(x => decodeURIComponent(x))

    const entities = [await getEntity(project, header, entity)]

    for (const i in members) {
        let entityI = Number(i)
        let targetName = members[entityI]
        let owner = entities[entities.length - 1]

        if (targetName == owner.name) {
            entities.push({ ...owner.constructor, name: owner.name, type: "function" })
        } else owner.memberGroups?.find((memberGroup) => {
            memberGroup.items.find((member) => {
                if (member.name === targetName) entities.push(member)

                return (entities.length > 1 + entityI)
            })
            return (entities.length > 1 + entityI)
        })
    }

    let data = {
        ...entities[entities.length - 1],
        project: entities[0].project,
        header: entities[0].header,
        owners: entities.slice(0, entities.length - 1).map(x => x.name),
        entity: entity,
        namespace: entities[0].namespace,
    }

    return <EntityViewer data={data} />
}

function getMembers(entity, members) {
    return (members ?? []).map(member => {
        let data = {
            ...entity,
            members: [...entity.members, betterEncodeURIComponent(member.name)]
        }
        return [
            data,
            ...getMembers(data, member.members)
        ]
    })
}
export async function generateStaticParams() {
    const overview = await getOverview()

    return overview.projects.map(project =>
        project.headers.map(header =>
            header.entities.map(entity =>
                getMembers({
                    project: betterEncodeURIComponent(project.name),
                    header: betterEncodeURIComponent(header.name),
                    entity: betterEncodeURIComponent(entity.name),
                    members: []
                }, entity.members)
            )
        )
    ).flat(Infinity)
}
