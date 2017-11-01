// A simple click counter component, can be a functional compontent as well
class Counter extends React.Component {
	constructor(props) {
		super(props)
	}
  render() {
    return (
      <div>
        <div>count:{this.props.value}</div>
				<button onClick={this.props.incr}>click!</button>
      </div>
    )
  }
}

// Render top-level component, pass controller data as props
const render = () =>
	ReactDOM.render(<Counter value={counter.data.value} incr={()=>counter.add(1)}/>,
									document.getElementById('app'));

// Call global render() when controller changes
counter.render = render;

// Render of the first time
render();
