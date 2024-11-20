import React, { useState } from "react";

const App = () => {
  const [value1, setValue1] = useState('');
  const [value2, setValue2] = useState('');
  const [result, setResult] = useState(null);

  const calculateSum = () => {
    const sum = parseFloat(value1 || 0) + parseFloat(value2 || 0);
    setResult(sum);
  };

  return (
    <div style={{ margin: '20px', fontFamily: 'Arial, sans-serif' }}>
      <h2>WebUI React Example</h2>
      <div style={{ marginBottom: '10px' }}>
        <label>
          <strong>Input 1:</strong>
        </label>
        <input
          type="number"
          value={value1}
          onChange={(e) => setValue1(e.target.value)}
          style={{ marginLeft: '10px', padding: '5px', fontSize: '16px' }}
        />
      </div>
      <div style={{ marginBottom: '10px' }}>
        <label>
          <strong>Input 2:</strong>
        </label>
        <input
          type="number"
          value={value2}
          onChange={(e) => setValue2(e.target.value)}
          style={{ marginLeft: '10px', padding: '5px', fontSize: '16px' }}
        />
      </div>
      <button
        onClick={calculateSum}
        style={{
          padding: '10px 20px',
          backgroundColor: '#4CAF50',
          color: 'white',
          fontSize: '16px',
          border: 'none',
          borderRadius: '5px',
          cursor: 'pointer',
          marginRight: '10px',
        }}
      >
        Calculate (React Frontend)
      </button>
      <button
        id="Exit"
        style={{
          padding: '10px 20px',
          backgroundColor: '#f44336',
          color: 'white',
          fontSize: '16px',
          border: 'none',
          borderRadius: '5px',
          cursor: 'pointer',
        }}
      >
        Exit (WebUI Backend)
      </button>
      <div style={{ marginTop: '20px', fontSize: '18px' }}>
        <strong>Result:</strong> {result !== null ? result : 'N/A'}
      </div>
    </div>
  );
};

export default App;
