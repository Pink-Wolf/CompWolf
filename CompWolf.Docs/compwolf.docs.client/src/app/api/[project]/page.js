import { getProject, getOverview } from "@/lib/api/getEntity"
import ProjectViewer from "@/lib/api/viewer/Project"
import betterEncodeURIComponent from "@/lib/betterEncodeURIComponent"

export default async function ProjectPage({ params }) {
    if (params.project === "%5Bproject%5D") return <div />
    const project = decodeURIComponent(params.project)

    const data = await getProject(project)

    return <ProjectViewer data={data} />
}

export async function generateMetadata({params}) {
    return {
        title: `${params.project}`,
    }
}

export async function generateStaticParams() {
    const overview = await getOverview()

    return overview.projects.map(project => {
        return {
            project: betterEncodeURIComponent(project.name)
        }
    })
}
