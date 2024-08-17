function BasicInput({ type, value, setter, disabled }) {
    return (
        <input type={type} value={value} disabled={disabled} onChange={(e) => setter(e.target.value)} />
    )
}
function TextAreaInput({ value, setter, disabled }) {
    return (
        <textarea value={value} disabled={disabled} onChange={(e) => setter(e.target.value)} />
    )
}
function SelectInput({ value, setter, disabled, options }) {
    return (
        <select value={value} disabled={disabled} onChange={(e) => setter(e.target.value)}>
            {options?.map((option, optionI) => {
                return <option key={optionI} value={option.value ?? option}>{option.name ?? option}</option>
            })}
        </select>
    )
}

function ArrayInput({ value, setter, disabled, forEach, newValue = () => "" }) {
    return (
        <ul>
            {value?.map((x, i) => {
                return (
                    <li key={i}>
                        <input type="button" value="-" disabled={disabled}
                            onClick={(e) => setter(value.toSpliced(i, 1))} />
                        {forEach(x, i, (newX) => setter(value.with(i, newX)))}
                    </li>
                )
            })}
            <li>
                <input type="button" value="+" disabled={disabled}
                    onClick={(e) => setter((value ?? []).concat(newValue()))} />
            </li>
        </ul>
    )
}

export function InputContainer({ value, field, setter, inputs }) {
    return (
        <section className="inputContainer">
            { inputs(value, setter) }
        </section>
    )
}

export default function Input(props) {
    var InputComponent = BasicInput
    var multipleInputs = false
    switch (props.type) {
        case "textarea": InputComponent = TextAreaInput
            break
        case "select": InputComponent = SelectInput
            break
        case "array": InputComponent = ArrayInput
            multipleInputs = true
            break
        case "container": InputComponent = InputContainer
            multipleInputs = true
        default: break
    }

    const label = props.label ?? (props.field
        ? `${props.field[0].toUpperCase() + props.field.slice(1).replace(/([A-Z]+)/g, ' $1')}:`
        : ""
    )

    const hasField = typeof props.field !== `undefined`

    const internalProps = {
        ...props,
        setter: hasField
            ? (x) => props.setter({ ...props.value, [props.field]: x })
            : props.setter,
        value: hasField
            ? props.value[props.field]
            : props.value
    }

    if (multipleInputs) return <section>{label}
        <InputComponent {...internalProps} />
    </section>

    return <label>{label}
        <InputComponent {...internalProps} />
    </label>
}

export function SubmitInput({ label }) {
    return <input type="submit" value={label} />
}