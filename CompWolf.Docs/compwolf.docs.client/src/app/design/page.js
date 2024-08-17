import Link from "next/link"

export default function Design() {
    return (
        <div>
            <h1>CompWolf Design</h1>
            <ul>
                <li><Link href="/design/standards">Standards</Link></li>
                <li><Link href="/design/use_cases">Use Cases</Link></li>
            </ul>

            <div>
                Games&apos; logic can generally be split into various generic sections:
                <ul>
                    <li>Core mechanics;</li>
                    <li>Cosmetics (visuals, sounds, etc.), including:
                        <ul>
                            <li>Camera,</li>
                            <li>HUD;</li>
                        </ul>
                    </li>
                    <li>Networking;</li>
                    <li>Player Controller;</li>
                    <li>Settings.</li>
                </ul>
            </div>
        </div>
    )
}
