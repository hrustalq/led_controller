import { apiInstance } from "../instance"

export default async () => {
  return await apiInstance.get("/hosts/all").then((response) => response.data);
}
