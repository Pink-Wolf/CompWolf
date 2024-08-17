import { getProject, getOverview } from "@/lib/api/getEntity"
import ProjectViewer from "@/lib/api/viewer/Project"
import betterEncodeURIComponent from "@/lib/betterEncodeURIComponent"

export default async function ProjectPage({ params }) {
    if (params.project === "%5Bproject%5D") return <div />
    let project = decodeURIComponent(params.project)

    const overview = await getOverview()
    const data = {
        ...(await getProject(project)),
        headers: overview
            .projects.find((x) => { return x.name === project })
            .headers
    }

    return <ProjectViewer data={data} />
}

export async function generateStaticParams() {
    const overview = await getOverview()

    return overview.projects.map(project => {
        return {
            project: betterEncodeURIComponent(project.name)
        }
    })
}
