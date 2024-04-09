import { Session } from '../entities/session.entity';

export class GetSessionsResponseDto {
  version: number | null;
  result: Session[];
  httpStatusCode: number;
  message: string | null;
  isError: boolean;
}
