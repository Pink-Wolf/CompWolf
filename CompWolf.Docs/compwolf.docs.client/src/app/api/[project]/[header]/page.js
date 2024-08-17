import { getHeader, getOverview } from "@/lib/api/getEntity"
import HeaderViewer from "@/lib/api/viewer/Header"
import betterEncodeURIComponent from "@/lib/betterEncodeURIComponent"

export default async function HeaderPage({ params }) {
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

    return <HeaderViewer data={data} />
}

export async function generateStaticParams() {
    const overview = await getOverview()

    return overview.projects.map(project =>
        project.headers.map(header => {
            return {
                project: betterEncodeURIComponent(project.name),
                header: betterEncodeURIComponent(header.name),
            }
        })
    ).flat(Infinity)
}
