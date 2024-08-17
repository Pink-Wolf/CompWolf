import { CodeViewer, SimpleReference } from "@/lib/CodeComponents"

export default function UseCases() {
    return (
        <div>
            <h1>CompWolf Use Cases</h1>
            <h2>0: Generalized</h2>
            <ol>
                <li>Create core (movement) mechanic</li>
                <li>Create test level</li>
                <li>Create core mechanics</li>
                <li>Create HUD</li>
                <li>Create content:
                    <ul>
                        <li>Levels</li>
                        <li>Items for core mechanics</li>
                    </ul>
                </li>
                <li>Create AI</li>
                <li>Add 3d models</li>
                <li>Logic external to core gameplay, potentially including:
                    <ul>
                        <li>Main menu</li>
                        <li>Options</li>
                        <li>Servers</li>
                        <li>Save system</li>
                    </ul>
                </li>
                <li>Create additional mechanics</li>
            </ol>
            <h2>1: Mystery Dungeon Creation</h2>
            <ol>
                <li>Create square players that can move + turn system</li>
                <li>Create walls</li>
                <li>Create attack system</li>
                <li>Create attacks</li>
                <li>Create health and death</li>
                <li>Create HUD</li>
                <li>Create level generator</li>
                <li>Create dungeon level system</li>
                <li>Create enemy AI</li>
                <li>Add 3d models</li>
                <li>Create player level system</li>
                <li>Create type and species/class system</li>
                <li>Create main menu</li>
                <li>Add save system</li>
                <li>Create options</li>
                <li>Add items.</li>
            </ol>
            <h2>2: Multiplayer FPS Creation</h2>
            <ol>
                <li>Create humanoid player that can move + test level</li>
                <li>Create dummy humanoid</li>
                <li>Create gun</li>
                <li>Create HUD</li>
                <li>Create health + death</li>
                <li>Set up multiplayer</li>
                <li>Create level</li>
                <li>Create enemy AI</li>
                <li>Add 3d models</li>
                <li>Create main menu</li>
                <li>Create server + connection system</li>
                <li>Create scoreboard</li>
                <li>Create options</li>
                <li>Create account system</li>
                <li>Create online save system</li>
                <li>Create buyable guns</li>
            </ol>
        </div>
    )
}
