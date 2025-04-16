import { getHeader, getOverview } from "@/lib/api/getEntity"
import HeaderViewer from "@/lib/api/viewer/Header"
import { generateStaticParamsEncoder, decodeStaticParamsEncoder } from "@/lib/betterEncodeURIComponent"

export default async function HeaderPage(input) {
    let params = await input.params
    if (params.project === "%5Bproject%5D") return <div />
    const project = decodeStaticParamsEncoder(params.project)
    const header = decodeStaticParamsEncoder(params.header)

    const data = await getHeader(project, header)

    return <HeaderViewer data={data} />
}

export async function generateMetadata({params}) {
    return {
        title: `${(await params).header}`,
    }
}

export async function generateStaticParams() {
    const overview = await getOverview()

    return overview.projects.map(project =>
        project.headers.map(header => {
            return {
                project: generateStaticParamsEncoder(project.name),
                header: generateStaticParamsEncoder(header.name),
            }
        })
    ).flat(Infinity)
}
