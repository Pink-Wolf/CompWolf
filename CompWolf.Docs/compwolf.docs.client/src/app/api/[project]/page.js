import { getProject, getOverview } from "@/lib/api/getEntity"
import ProjectViewer from "@/lib/api/viewer/Project"
import { generateStaticParamsEncoder, decodeStaticParamsEncoder } from "@/lib/betterEncodeURIComponent"

export default async function ProjectPage(input) {
    let params = await input.params
    if (params.project === "%5Bproject%5D") return <div />
    const project = decodeStaticParamsEncoder(params.project)

    const data = await getProject(project)

    return <ProjectViewer data={data} />
}

export async function generateMetadata({params}) {
    return {
        title: `${(await params).project}`,
    }
}

export async function generateStaticParams() {
    const overview = await getOverview()

    return overview.projects.map(project => {
        return {
            project: generateStaticParamsEncoder(project.name)
        }
    })
}
