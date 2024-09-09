function BasicInput({ type, value, setter, disabled, ...props }) {
    return (
        <input {...props} type={type} value={value} disabled={disabled}
            onChange={(e) => setter(e.target.value)}
        />
    )
}
function TextAreaInput({ value, setter, disabled, ...props }) {
    return (
        <textarea {...props} value={value} disabled={disabled}
            onChange={(e) => setter(e.target.value)}
        />
    )
}
function SelectInput({ value, setter, disabled, options, ...props }) {
    return (
        <select {...props} value={value} disabled={disabled}
            onChange={(e) => setter(e.target.value)}
        >
            {options?.map((option, optionI) => {
                return <option key={optionI} value={option.value ?? option}>{option.name ?? option}</option>
            })}
        </select>
    )
}
function CheckboxInput({ value, setter, disabled, ...props }) {
    return (
        <input {...props} type="checkbox" checked={value} disabled={disabled}
            onChange={(e) => setter(e.target.checked)}
        />
    )
}

function ArrayInput({ value, setter, disabled, forEach, newValue = () => "", ...props }) {
    return (
        <ul {...props}>
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

export function InputContainer({ value, field, setter, inputs, ...props }) {
    return (
        <section {...props} className="inputContainer">
            { inputs(value, setter) }
        </section>
    )
}

export default function Input({ type, value, setter, label, field, ...props }) {
    var InputComponent = BasicInput
    var multipleInputs = false
    switch (type) {
        case "textarea": InputComponent = TextAreaInput
            break
        case "select": InputComponent = SelectInput
            break
        case "boolCheckbox": InputComponent = CheckboxInput
            break;
        case "array": InputComponent = ArrayInput
            multipleInputs = true
            break
        case "container": InputComponent = InputContainer
            multipleInputs = true
            break;
        default: break
    }

    label = label ?? (field
        ? `${field[0].toUpperCase() + field.slice(1).replace(/([A-Z]+)/g, ' $1')}:`
        : ""
    )

    const hasField = typeof field !== `undefined`

    const internalProps = {
        ...props,
        setter: hasField
            ? (x) => setter({ ...value, [field]: x })
            : setter,
        value: hasField
            ? value[field]
            : value
    }

    if (multipleInputs) return <section className="labelSection">{label}
        <InputComponent {...internalProps} />
    </section>

    return <label>{label}
        <InputComponent {...internalProps} />
    </label>
}

export function SubmitInput({ label }) {
    return <input type="submit" value={label} />
}