import axios from "axios";

import hostsApi from "./hosts";

export const apiInstance = axios.create({
  baseURL: "http://localhost:3001",
  headers: {
    "Content-Type": "application/json",
  },
});


export default {
  ...hostsApi,
}
